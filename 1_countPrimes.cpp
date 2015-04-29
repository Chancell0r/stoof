#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <iostream> 


int countPrimes(int n);

int main(int argc, char** argv ){
	int input = atoi(argv[1]);

	int result = countPrimes(input);

	std::cout << "Count of Primes Below " <<  argv[1] << " is: " << result << "\n";
}

int countPrimes(int n) {
	int count = 0;
	bool isPrime[n+1];
    
    for (int i = 2; i <= n; i++) {
        isPrime[i] = true;
    }
    
    for(int i = 2; i*i <= n; i++){
        if(isPrime[i]){
            for(int j = i; i*j <= n; j++){
                isPrime[i*j] = false;
            }
        }
    }
    for(int i = 2; i <= n -1; i++){
        if(isPrime[i]) count++;
    }

    return count;
}
