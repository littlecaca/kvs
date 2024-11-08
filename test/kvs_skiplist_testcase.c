#include "kvs_skiptalist.h"

#include <assert.h>
#include <stdio.h>

int main() {
    skiplist *engine = skiplist_create();

	skiplist_insert(engine, "Teacher1", "King");
	skiplist_insert(engine, "Teacher2", "Darren");
	skiplist_insert(engine, "Teacher3", "Mark");
	skiplist_insert(engine, "Teacher4", "Vico");
	skiplist_insert(engine, "Teacher5", "Nick");

	skiplist_node *value1 = skiplist_search(engine, "Teacher1");
    assert(value1 != NULL);
	printf("Teacher1 : %s\n", value1->value);

	int ret = skiplist_modify(engine, "Teacher1", "King1");
    assert(ret == 0);
	printf("mode Teacher1 ret : %d\n", ret);
	
	value1 = skiplist_search(engine, "Teacher2");
    assert(value1 != NULL);
	printf("Teacher2 : %s\n", value1->value);

	skiplist_delete(engine, "Teacher2");

	ret = skiplist_exist(engine, "Teacher2");
    assert(ret == 0);
	printf("Exist Teacher1 ret : %d\n", ret);

	skiplist_destroy(engine);

	return 0;
}
