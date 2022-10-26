user.a: user/user.a common/common.a
	$(AR) -r $@ $^

user/user.a:
	make -C user

common/common.a:
	make -C common
