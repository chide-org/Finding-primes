# -*- coding: utf-8 -*-

import pymysql
import time
import math
import csv
from dbutils.persistent_db import PersistentDB

# 创建一个数据库连接池
pool = PersistentDB(pymysql,
                    host="localhost",
                    port=3306,
                    user="admin",
                    password="******",
                    database="prime",
                    charset='utf8')


def is_prime(n):
    if n <= 1:
        return False
    elif n <= 3:
        return True
    elif n % 2 == 0 or n % 3 == 0:
        return False
    i = 5
    while i * i <= n:
        if n % i == 0 or n % (i + 2) == 0:
            return False
        i += 6
    return True


def save_prime_to_database(prime, time_taken):
    connection = pool.connection()
    cursor = connection.cursor()
    try:
        current_time = time.strftime('%Y-%m-%d %H:%M:%S')
        query = "INSERT INTO primes (prime_number, time_taken, timestamp) VALUES (%s, %s, %s)"
        cursor.execute(query, (prime, time_taken, current_time))
        connection.commit()

    except pymysql.Error as error:
        print("Error saving prime number to database:", error)

    finally:
        cursor.close()
        connection.close()


def save_primes_to_csv(primes, filename,alltime):
    with open(filename, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["compute time:", alltime])
        writer.writerow(["first:", primes[0]])
        writer.writerow(["last:", primes[-1]])
        # Write 100 rows of 50 columns
        for i in range(0, len(primes), 50):
            writer.writerow(primes[i:i+50])


def main():
    start_time = time.time()
    time5000 = time.time()
    prime_count = 0
    current_number = 2
    primes = []

    while True:
        if is_prime(current_number):
            prime_count += 1
            end_time = time.time()
            time_taken = end_time - start_time
            save_prime_to_database(current_number, time_taken)
            primes.append(current_number)

            # Check if we have found 5000 primes
            if prime_count == 5000:
                filename = f'prime_{time.strftime("%Y%m%d_%H%M%S")}.csv'
                alltime = time.time()-time5000
                save_primes_to_csv(primes, filename,alltime)
                primes.clear()  # Clear the list after saving to CSV
                prime_count = 0
                time5000 = time.time()
                print(f"Saved {prime_count} primes to {filename}")

        current_number += 1


if __name__ == "__main__":
    main()
