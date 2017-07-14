#ifndef SCULL_H
#define SCULL_H

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0
#endif

#ifndef SCULL_MINOR 
#define SCULL_MINOR 0
#endif

#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4096
#endif

#ifndef SCULL_QSET
#define SCULL_QSET 1000
#endif

struct scull_qset {
	void **data;
	struct scull_qset *next;
};

typedef struct scull_dev {
	struct scull_qset *data;
	struct scull_dev *next;
	int quantum;
	int qset;
	unsigned long size;

	struct cdev cdev;
};

#endif
