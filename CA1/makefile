

DEPS = defs.h network.h network.c cJSON.c cJSON.h Food.c custom_string.c 

checkFolder:
	if [ ! -d "./logs" ]; then \
		mkdir -p ./logs; \
	fi

all: checkFolder Restaurant.out Customer.out Supplier.out

Restaurant.out: Restaurant.c $(DEPS)
	gcc Restaurant.c cJSON.c -o Restaurant.out
Customer.out: Customer.c $(DEPS)
	gcc Customer.c cJSON.c -o Customer.out
Supplier.out: Supplier.c $(DEPS)
	gcc Supplier.c -o Supplier.out

	

clean:
	rm -f logs/*.txt
	find . -name "*.out" -type f -delete
