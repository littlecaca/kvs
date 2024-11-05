#include "kvs_hash.h"

#include <assert.h>
#include <stdio.h>

int main() {
    kvs_hash *engine = kvs_hash_create();

	kvs_hash_insert(engine, "Teacher1", "King");
	kvs_hash_insert(engine, "Teacher2", "Darren");
	kvs_hash_insert(engine, "Teacher3", "Mark");
	kvs_hash_insert(engine, "Teacher4", "Vico");
	kvs_hash_insert(engine, "Teacher5", "Nick");

	kvs_hash_node *value1 = kvs_hash_search(engine, "Teacher1");
    assert(value1 != NULL);
	printf("Teacher1 : %s\n", value1->value);

	int ret = kvs_hash_modify(engine, "Teacher1", "King1");
    assert(ret == 0);
	printf("mode Teacher1 ret : %d\n", ret);
	
	value1 = kvs_hash_search(engine, "Teacher2");
    assert(value1 != NULL);
	printf("Teacher2 : %s\n", value1->value);

	kvs_hash_delete(engine, value1);

	ret = kvs_hash_exist(engine, "Teacher2");
    assert(ret == 0);
	printf("Exist Teacher1 ret : %d\n", ret);

	kvs_hash_destroy(engine);

	return 0;
}
