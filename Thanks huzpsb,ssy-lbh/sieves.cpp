#include "sieves.h"

#include "number_theory.h"
#include "big_num.h"

uint32_t euler_sieve(uint32_t n, uint32_t fac[], uint32_t prime[]){
    uint32_t cnt = 0;
    for (uint32_t i = 2; i < n; i++){
        if (!fac[i]){
            fac[i] = i;
            prime[cnt++] = i;
        }
        uint32_t tmp = (n - 1) / i; // tmp = ceil(n / i) - 1
        if (tmp > fac[i])
            tmp = fac[i];
        for (uint32_t j = 0; j < cnt; j++){
            if (prime[j] > tmp) break;
            fac[i * prime[j]] = prime[j];
        }
    }
    return cnt;
}

uint64_t euler_sieve(uint64_t n, uint64_t fac[], uint64_t prime[]){
    uint64_t cnt = 0;
    for (uint64_t i = 2; i < n; i++){
        if (!fac[i]){
            fac[i] = i;
            prime[cnt++] = i;
        }
        uint64_t tmp = (n - 1) / i; // tmp = ceil(n / i) - 1
        if (tmp > fac[i])
            tmp = fac[i];
        for (uint64_t j = 0; j < cnt; j++){
            if (prime[j] > tmp) break;
            fac[i * prime[j]] = prime[j];
        }
    }
    return cnt;
}

uint64_t euler_sieve(uint64_t n, uint64_t fac[], uint64_t prime[], uint64_t* phi, uint64_t* mobius){
    uint64_t cnt = 0;
    if (phi) phi[1] = 1;
    if (mobius) mobius[1] = 1;
    for (uint64_t i = 2; i < n; i++){
        if (!fac[i]){
            fac[i] = i;
            prime[cnt++] = i;
            if (phi) phi[i] = i - 1;
            if (mobius) mobius[i] = 1;
        }
        uint64_t tmp = (n - 1) / i; // tmp = ceil(n / i) - 1
        if (tmp > fac[i])
            tmp = fac[i];
        for (uint64_t j = 0; j < cnt; j++){
            if (prime[j] > tmp) break;
            fac[i * prime[j]] = prime[j];
            if (prime[j] == fac[i]){ // prime[j] | i
                if (phi) phi[i * prime[j]] = phi[i] * prime[j];
                if (mobius) mobius[i * prime[j]] = 0;
                break;
            }
            if (phi) phi[i * prime[j]] = phi[i] * (prime[j] - 1);
            if (mobius) mobius[i * prime[j]] = -mobius[i];
        }
    }
    return cnt;
}

uint32_t egypt_sieve(uint32_t n, bool tag[], uint32_t prime[]){
    uint32_t cnt = 0;
    for (uint32_t i = 2; i < n; i++){
        if (!tag[i]){
            prime[cnt++] = i;
            uint64_t tmp = i * i;
            if (tmp >= n) continue;
            for (uint64_t j = tmp; j < n; j += i)
                tag[j] = true;
        }
    }
    return cnt;
}

uint64_t egypt_sieve(uint64_t n, bool tag[], uint64_t prime[]){
    uint64_t cnt = 0;
    for (uint64_t i = 2; i < n; i++){
        if (!tag[i]){
            prime[cnt++] = i;
            if (i > (n - 1) / i)
                continue;
            for (uint64_t j = i * i; j < n; j += i)
                tag[j] = true;
        }
    }
    return cnt;
}

// min25筛 O(n^{2/3}) | O(\frac{n^{3/4}}{\log n})

// 杜教筛 O(n^{2/3})
// 求解 \sum_{i=1}^n f(i)
// 输入 h = \sum{i=1}^n f * g
// 默认 g = 1

// 常用公式 f * g = h
// \phi * 1 = id
// \mu * 1 = \epsilon ([x == 1])
// id_k * 1 = \sigma_k, \sigma_k * \mu = id_k
// \phi * \sigma_0 = id * 1 = \sigma_1

// 莫比乌斯反演: f = 1 * g <=> g = \mu * f

int64_t dujiao_sum(uint64_t x, std::unordered_map<uint64_t, int64_t>& cache, std::function<int64_t(uint64_t)> hsum){
    if (cache.count(x)) return cache[x];
    int64_t res = hsum(x);
    for (uint64_t l = 2, r; l <= x; l = r + 1){
        uint64_t y = x / l;
        // r = x / (x / l), [x / n] == y 求出的n区间
        r = x / y;
        res -= (r - l + 1) * dujiao_sum(y, cache, hsum); // 递归求解, r - l + 1 为函数 g 的区间和
    }
    return cache[x] = res;
}

int64_t dujiao_sieve(uint64_t n, std::function<int64_t(uint64_t)> hsum){
    std::unordered_map<uint64_t, int64_t> cache;
    return dujiao_sum(n, cache, hsum);
}

int64_t epsilon_sum(uint64_t x){ return 1; }
int64_t I_sum(uint64_t x){ return x; }
int64_t id_sum(uint64_t x){ return x * (x + 1) / 2; }
int64_t phi_sum(uint64_t x){ return dujiao_sieve(x, id_sum); }
int64_t mobius_sum(uint64_t x){ return dujiao_sieve(x, epsilon_sum); }

int64_t sigma0_sum(uint64_t x){
    // sigma0 * mobius = I
    std::unordered_map<uint64_t, int64_t> mobius_cache;
    dujiao_sum(x, mobius_cache, epsilon_sum);
    return dujiao_sieve(x, [&](uint64_t x){ return mobius_cache[x]; });
}

// len(tmp) = pcnt + maxm + 4 * maxq
BigInt quadratic_sieve(BigInt n, uint64_t prime[], uint64_t tmp[], uint64_t pcnt, uint64_t cand_num) {
    uint64_t maxq = pcnt + 50;

	uint64_t qtot = 1;
    tmp[0] = 2;
	for(uint64_t i = 1; i < pcnt; i++)
		if(pow(uint64_t(n % prime[i]), (prime[i] - 1) >> 1, prime[i]) == 1)
			tmp[qtot++] = prime[i];
	BigInt sqn = sqrt(n) + 1;

    BigInt* seq = new BigInt[cand_num];
    BigInt* num = new BigInt[maxq];
    BigInt* ex = new BigInt[maxq];
    BigInt* msk = new BigInt[maxq];
    BigInt* sel = new BigInt[maxq];
    
	seq[0] = sqn * sqn - n;
	for(int i = 0; i < cand_num; ++i)
		seq[i + 1] = seq[i] + ((sqn + i) << 1 | 1);
	for(int i = 0; i < qtot; ++i) {
		uint64_t pp = tmp[i];
        uint64_t rem = uint64_t((sqn * sqn - n) % pp);
        uint64_t sq = uint64_t(sqn % pp);
		for(uint64_t j = 0; j < pp; j++) {
			if(!rem)
				for(int k = j; k < cand_num; k += pp)
					seq[k] /= pp;
			rem = (rem + ((sq + j) << 1 | 1)) % pp;
		}
	}
	uint64_t stot = 0;
	BigInt cur = sqn * sqn - n;
	for(uint64_t i = 0; i < cand_num && stot < maxq; i++) {
		if(seq[i] == 1) {
			BigInt t = cur;
			for(uint64_t j = 0; j < qtot; j++)
				if(t % tmp[j] == 0) {
					ex[stot].setbit(j);
                    msk[stot].setbit(j);
					t /= tmp[j];
				}
			sel[stot] = 0;
			sel[stot].setbit(stot);
			num[stot++] = sqn + i;
		}
		cur += (sqn + i) << 1 | 1;
	}
    delete[] seq;
	uint64_t rk = 0;
	for(uint64_t i = 0; i < qtot; i++) {
		uint64_t k = -1;
		for(uint64_t j = rk; j < stot; j++)
			if(msk[j].testbit(i)) {
				k = j;
				break;
			}
		if(k == -1)
			continue;
		msk[rk].swap(msk[k]);
        sel[rk].swap(sel[k]);
		for(uint64_t j = 0; j < stot; j++)
			if(j != rk && msk[j].testbit(i)) {
				msk[j] ^= msk[rk];
				sel[j] ^= sel[rk];
			}
		rk++;
	}
    delete[] msk;
    delete[] sel;
    uint64_t* cnt = new uint64_t[pcnt];
	for(uint64_t i = rk; i < stot; i++) {
		BigInt x = 1, y = 1;
		memset(cnt, 0, sizeof(cnt));
		for(uint64_t j = 0; j < stot; j++)
			if(sel[i].testbit(j)) {
				x = mul(x, num[j], n);
				for(uint64_t k = 0; k < qtot; k++)
					cnt[k] += ex[j].testbit(k);
			}
		for(uint64_t j = 0; j < qtot; j++)
			y = mul(y, pow(BigInt(tmp[j]), BigInt(cnt[j] >> 1), n), n);
		BigInt u = gcd(x + y < n ? x + y : x + y - n, n);
		if(u > 1 && u < n)
			return u;
		BigInt v = gcd(x < y ? x - y + n : x - y, n);
		if(v > 1 && v < n)
			return v;
	}
    delete[] ex;
    delete[] cnt;
	return n;
}
