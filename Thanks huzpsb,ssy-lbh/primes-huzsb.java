import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.atomic.AtomicInteger;

public class Prime {
    private static volatile ArrayList<Integer> foundPrimes = new ArrayList<>();
    private static final ArrayList<Integer> cache = new ArrayList<>();
    private static final int processors = Runtime.getRuntime().availableProcessors();
    private static int cacheSize = 100;

    private static synchronized void reportPrime(int i) {
        if (i != -1) {
            cache.add(i);
        }
        if (cache.size() == cacheSize || i == -1) {
            Arrays.sort(cache.toArray());
            ArrayList<Integer> copy = new ArrayList<>(foundPrimes);
            copy.addAll(cache);
            foundPrimes = copy;
            cacheSize = foundPrimes.size() * 10;
            cache.clear();
        }
    }

    public static void main(String[] args) throws Exception {
        long start = System.currentTimeMillis();

        // Let's gen 100 primes with brute force
        foundPrimes.add(2);
        int i = 3;
        while (true) {
            for (int prime : foundPrimes) {
                if (i % prime == 0) {
                    break;
                }
                long square = (long) prime * prime;
                if (square > i) {
                    foundPrimes.add(i);
                    break;
                }
            }
            if (foundPrimes.size() == 100) {
                break;
            }
            i += 2;
        }
        // Now let's pool it up
        AtomicInteger index = new AtomicInteger(i);
        final int max = Integer.MAX_VALUE - processors * 5;
        final CountDownLatch latch = new CountDownLatch(processors);
        for (int t = 0; t < processors; t++) {
            new Thread(() -> {
                while (true) {
                    int work = index.addAndGet(2);
                    if (work % 100000000 == 1) {
                        System.out.println("Current: " + work);
                    }
                    if (work > max) {
                        latch.countDown();
                        break;
                    }
                    boolean exitedNormally = false;
                    for (int prime : foundPrimes) {
                        if (work % prime == 0) {
                            exitedNormally = true;
                            break;
                        }
                        long square = (long) prime * prime;
                        if (square > work) {
                            exitedNormally = true;
                            reportPrime(work);
                            break;
                        }
                    }

                    if (!exitedNormally) {
                        System.err.println("!Can not check this number: " + work);
                        System.exit(1);
                    }
                }
            }).start();
        }
        latch.await();

        reportPrime(-1);
        BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream("primes.txt"));
        PrintWriter writer = new PrintWriter(bos);
        for (int prime : foundPrimes) {
            writer.println(prime);
        }
        writer.close();

        System.out.println("Time: " + (System.currentTimeMillis() - start) + "ms");
    }
}