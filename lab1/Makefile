CC = mpicc

TASK1_SRC = src/task1.c
TASK2_SRC = src/task2.c

TASK1_TARGET=task1
TASK2_TARGET=task2

BUILD_DIR = build

PROCESSES = 8

.PHONY: all graph clean

all: ${TASK1_TARGET} ${TASK2_TARGET}

${TASK1_TARGET}: ${TASK1_SRC}
	@${CC} -DVERBOSE ${TASK1_SRC} -o ${TASK1_TARGET}

${TASK2_TARGET}: ${TASK2_SRC}
	@${CC} -DVERBOSE ${TASK2_SRC} -o ${TASK2_TARGET}

graph:
	if [ ! -d "venv" ]; then \
		python -m venv venv; \
		source venv/bin/activate; \
		pip install -r requirements.txt; \
	else \
		source venv/bin/activate; \
	fi; \
	TMP_DIR=$$(mktemp -d); \
	${CC} -DMEASURE_TIME ${TASK1_SRC} -o $$TMP_DIR/${TASK1_TARGET}; \
	${CC} -DMEASURE_TIME ${TASK2_SRC} -o $$TMP_DIR/${TASK2_TARGET}; \
	python3 src/make_graph.py graph.png $$TMP_DIR/${TASK1_TARGET} $$TMP_DIR/${TASK2_TARGET}

clean:
	rm ${TASK1_TARGET} ${TASK2_TARGET}
