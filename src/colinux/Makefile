all: user.a

user.a: user/user.a common/common.a
	$(CC) $^ -o $@

user/user.a:
	make -C user

common/common.a:
	make -C common
