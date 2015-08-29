


mydu(const char *fname)
{	// ../../a/b/c/d/e/f/xxxx/ccc/zzz

	//fname为非目录文件
		return    ;

	//fname为目录文件


#if 0

	glob("fname/*",,,&globres)
	for()
		sum += mydu(globres.gl_pathv[i]);
	
	glob("fname/.*",,,&globres);
	for()
        sum += mydu(globres.gl_pathv[i]);

#endif

	glob("fname/*",,,&globres);

	glob("fname/.*",,GLOB_APPEND,&globres);


	for()
	{
		if()	
       		sum += mydu(globres.gl_pathv[i]);
	}
	
	return sum;	

}

int main(int argc,char **argv)
{

	
	printf("",mydu(argv[i])/2);


}




