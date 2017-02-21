#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rados/librados.h>

int main(int argc, char *argv[])
{
	rados_t cluster;
	char cluster_name[] = "ceph";
	char user_name[] = "client.admin";
	uint64_t flags;
	rados_ioctx_t io;
	char *poolname = "rbd";
	char read_res[100];
	char xattr[] = "en_us";
	int err;

	err = rados_create2(&cluster, cluster_name, user_name, flags);
	if (err < 0) {
		printf("%s: Couldn't create the cluster handle: %d\n", argv[0], err);
		exit(-1);
	} else {
		printf("Created a cluster handle.\n");
	}

	err = rados_conf_read_file(cluster, "/etc/ceph/ceph.conf");
	if (err < 0) {
		printf("%s: Couldn't read config file: %d\n", argv[0], err);
		exit(-1);
	} else {
		printf("Read the config file.\n");
	}

	err = rados_conf_parse_argv(cluster, argc, (const char **)argv);
	if (err < 0) {
		printf("%s: Couldn't parse command line arguments: %d\n", argv[0], err);
		exit(-1);
	} else {
		printf("Read the command line arguments.\n");
	}

	err = rados_connect(cluster);
	if (err < 0) {
		printf("%s: Couldn't connect to cluster: %d\n", argv[0], err);
		exit(-1);
	} else {
		printf("Connect to the cluster.\n");
	}

	err = rados_ioctx_create(cluster, poolname, &io);
	if (err < 0) {
		printf("%s: Couldn't open rados pool %s: %d\n", argv[0], poolname, err);
		exit(-1);
	} else {
		printf("Create I/O context.\n");
	}

	err = rados_write(io, "hw", "Hello World!\n", 13, 0);
	if (err < 0) {
		printf("%s: Couldn't write object \"hw\" to pool %s: %d\n", argv[0], poolname, err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Wrote \"Hello World!\" to object \"hw\".\n");
	}

	err = rados_setxattr(io, "hw", "lang", xattr, 5);
	if (err < 0) {
		printf("%s: Couldn't set xattr to pool %s: %d\n", argv[0], poolname, err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Write \"en_US\" to xattr \"lang\" for object \"hw\".\n");
	}

	rados_completion_t comp;
	err = rados_aio_create_completion(NULL, NULL, NULL, &comp);
	if (err < 0) {
		printf("%s: Couldn't create aio completion: %d\n", argv[0], err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Created AIO completion.\n");
	}

	err = rados_aio_read(io, "hw", comp, read_res, 12, 0);
	if (err < 0) {
		printf("%s: Couldn't read object: %s %d\n", argv[0], poolname, err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Read object \"hw\". The contents are: %s.\n", read_res);
	}

/*
	rados_wait_for_complete(comp);
*/
	rados_aio_wait_for_complete(comp);
	printf("Read object \"hw\". The contents are: %s.\n", read_res);

	rados_aio_release(comp);

	char xattr_res[100];
	err = rados_getxattr(io, "hw", "lang", xattr_res, 5);
	if (err < 0) {
		printf("%s: Couldn't read xattr: %s %d\n", argv[0], poolname, err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Read xattr \"lang\" for object\"hw\". The contents are: %s.\n", xattr_res);
	}

	err = rados_rmxattr(io, "hw", "lang");
	if (err < 0) {
		printf("%s: Couldn't remove xattr: %s %d\n", argv[0], poolname, err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Removed xattr \"lang\" for object\"hw\".\n");
	}

	err = rados_remove(io, "hw");
	if (err < 0) {
		printf("%s: Couldn't remove object: %s %d\n", argv[0], poolname, err);
		rados_ioctx_destroy(io);
		rados_shutdown(cluster);
		exit(-1);
	} else {
		printf("Removed object \"hw\".\n");
	}

	rados_ioctx_destroy(io);
	rados_shutdown(cluster);

	return 0;
}
