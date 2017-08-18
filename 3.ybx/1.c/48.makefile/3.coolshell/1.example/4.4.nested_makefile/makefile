all:subsystem1 subsystem2
#执行方法最好:make -s

export var:=11111111111111111111111111111

subsystem1:
	@cd subdir && $(MAKE)

subsystem2:
	@$(MAKE) -C subdir
