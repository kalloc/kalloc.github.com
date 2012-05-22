/*
 * Tail queue definitions.
 */
#define TAILQ_HEAD(name, type)						\
struct name {								\
	struct type *tqh_first;	/* first element */			\
	struct type **tqh_last;	/* addr of last next element */		\
}

#define TAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* next element */			\
	struct type **tqe_prev;	/* address of previous next element */	\
}

/*
 * Tail queue functions.
 */
#define	TAILQ_INIT(head) {						\
	(head)->tqh_first = NULL;					\
	(head)->tqh_last = &(head)->tqh_first;				\
}

#define TAILQ_INSERT_HEAD(head, elm, field) {				\
	if (((elm)->field.tqe_next = (head)->tqh_first) != NULL)	\
		(elm)->field.tqe_next->field.tqe_prev =			\
		    &(elm)->field.tqe_next;				\
	else								\
		(head)->tqh_last = &(elm)->field.tqe_next;		\
	(head)->tqh_first = (elm);					\
	(elm)->field.tqe_prev = &(head)->tqh_first;			\
}

#define TAILQ_INSERT_TAIL(head, elm, field) {				\
	(elm)->field.tqe_next = NULL;					\
	(elm)->field.tqe_prev = (head)->tqh_last;			\
	*(head)->tqh_last = (elm);					\
	(head)->tqh_last = &(elm)->field.tqe_next;			\
}

#define TAILQ_INSERT_AFTER(head, listelm, elm, field) {			\
	if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL)\
		(elm)->field.tqe_next->field.tqe_prev = 		\
		    &(elm)->field.tqe_next;				\
	else								\
		(head)->tqh_last = &(elm)->field.tqe_next;		\
	(listelm)->field.tqe_next = (elm);				\
	(elm)->field.tqe_prev = &(listelm)->field.tqe_next;		\
}

#define TAILQ_REMOVE(head, elm, field) {				\
	if (((elm)->field.tqe_next) != NULL)				\
		(elm)->field.tqe_next->field.tqe_prev = 		\
		    (elm)->field.tqe_prev;				\
	else								\
		(head)->tqh_last = (elm)->field.tqe_prev;		\
	*(elm)->field.tqe_prev = (elm)->field.tqe_next;			\
}

// HELPER DEFINITION

#define TQ_DEFINE(name)							\
	TAILQ_HEAD(name ## _tailhead, name ## _entry) name ## _head;	\
	struct name ## _tailhead *name ## _headp;			\
	struct name ## _entry {						\
		TAILQ_ENTRY(name ## _entry) entries;

#define TQ_DEFINE_END(name)						\
	} *name ## _foreach;

#define TQ_REMOVE(name,ptr)						\
	TAILQ_REMOVE(&name ## _head, ptr, entries);

#define TQ_UP(name,ptr)							\
	TAILQ_REMOVE(&name ## _head, ptr, entries); 			\
	TAILQ_INSERT_TAIL(&name ## _head, ptr, entries);

#define TQ_INSERT(name,ptr)						\
	TAILQ_INSERT_TAIL(&name ## _head, ptr, entries);

#define TQ_VAR(name)							\
	struct name ## _entry

#define TQ_MALLOC(name)							\
	malloc(sizeof(struct name ## _entry));

#define TQ_INIT(name)							\
	TAILQ_INIT(&name ## _head);

#define TQ_EXPIRE(name)							\
	while ( (name ## _foreach = name ## _head.tqh_first) )

#define TQ_FOREACH(name)						\
	for (name ## _foreach = name ## _head.tqh_first;		\
		name ## _foreach != NULL; 				\
		name ## _foreach = name ## _foreach->entries.tqe_next)

#define TQ_ITEM(name)							\
	name ## _foreach

