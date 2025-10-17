CC = mpicc

TARGET = array_max
SOURCES = src/main.c

.PHONY: build_dir clean

all: ${TARGET}

${TARGET}: build_dir ${SOURCES}
	${CC} ${SOURCES} -o ${TARGET}

test:
	@python3 src/algo_test.py ${TARGET}

graph:
	if [ ! -d "venv" ]; then \
		python -m venv venv; \
		source venv/bin/activate; \
		pip install -r requirements.txt; \
	else \
		source venv/bin/activate; \
	fi; \
	TMP_DIR=$$(mktemp -d); \
	${CC} -DMEASURE_TIME ${SOURCES} -o $$TMP_DIR/${TARGET}; \
	gcc -DMEASURE_TIME src/simple_max.c -o $$TMP_DIR/simple_max; \
	python3 src/make_graph.py graph.png $$TMP_DIR/${TARGET} $$TMP_DIR/simple_max

clean:
	rm ${TARGET} graph.png venv __pycache__
