noinst_LIBRARIES=libpro.a  #生成的静态库文件名称，noinst加上之后是只编译，不安装到系统中。
libpro_a_SOURCES=sum.c get.c val.c #这个静态库文件需要用到的依赖
include_HEADERS=../include/common.h ../include/sum.h ../include/get.h ../include/val.h #导入需要依赖的头文件
#说明：src/目录下面不加include_HEADERS也是可以运行的，但是在使用make dist打包命令后，
#并不会将include/文件夹打包进去，所以还是需要加上include_HEADERS。
