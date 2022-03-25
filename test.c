#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define _OPEN_SYS_ITOA_EXT

struct _dir 
{
	char d_path[PATH_MAX];
	int  d_mode;
};
struct _file
{
	char f_path[PATH_MAX];
	char f_name[PATH_MAX];
	int  f_mode;
	unsigned int f_size;
};

void Trent 		(DIR *where, char * directory, char * Path_kat);	//Packing Files
char check_cond (const int  _type,const char * name );	//Check Dir
char check_file (const int  _type,const  char * name );	//Check File
char * Find_name(const char *_Adress);	//Find deepest directory in the Path 
char * _makePath(const char * _path);	//Create path
int _MDwrite 	(const struct dirent * _file, const char * _path);	//Record metaData into File
void _Dwrite	(const char *_path, unsigned _size);				//Record Data into File
void _MDirWr	(const char *_dir, const char *_path);				//Record current Directory
void _recDir(struct _dir * direct);
void _recFile(struct _file *fl);
void _dataFile(const int file,unsigned size);
int Unpacking ();

char New_Home_Path[PATH_MAX];
char Ar_Path[PATH_MAX];
char Adress[PATH_MAX];
int File_arch;
struct stat exite = {0};	

int main(int argc, char *argv[])
{
	DIR *in;
	if (argc<4)
		{
			perror("Not enought arguments\n");
			exit(0);
		}
	printf("%s\n",argv[0]);
	printf("%s\n",argv[1]);
	printf("%s\n",argv[2]);
	printf("%s\n",argv[3]);
	if (strcmp(argv[1], "AH") == 0)
	{
		char str[PATH_MAX];	//Path_pack	
		strcpy(str,argv[2]);
		strcpy(Ar_Path,argv[3]);		//Path_to_Arch
		in=opendir(str);	//Openning directory
		strcpy(Adress,Find_name(str));	//Creating path packed directory 
		if (!(File_arch=open(Ar_Path, (O_CREAT | O_RDWR | O_TRUNC), 0777) ) )	//Create_Arch_File
			perror("Error Arch file");
		if (in==NULL) 
		{
			perror("ErrorxD");
			exit(-1);
		}
		printf("ZARABOTALO!\n");
		Trent(in,str,Adress+1); // Function 'ls' with packing files 
	}
 	else if (strcmp(argv[1], "unAH") == 0)
		{
			strcpy(New_Home_Path,argv[2]);	//Path_unpack
			if (stat(New_Home_Path,&exite)== -1) 	//Create_Dir_unpacking
				mkdir(New_Home_Path, 0777);
			strcpy(Ar_Path,argv[3]);
			if (!(File_arch=open(Ar_Path,O_RDWR ) )	)//Create_Arch_File
				perror("Error Arch file");
	/*
		File_arch
		Ar_Path
		New_Home_Path
	*/
			printf("UNPACKING MODE ON\n");
			Unpacking();
		}
		else 
		{
			perror("Unknown command\n");
			exit(0);
		}	
	
	if (close(File_arch)==-1)
		perror("Error with closing arch file");
	exit(0);
} 
int Unpacking ()
{
	char flag;
	read(File_arch, &flag, 1);
	struct _dir D;
	struct _file F;
	if ( flag == 'D')
	{
		strcat(strcat(D.d_path,New_Home_Path),"/");
		_recDir(&D);
		mkdir(D.d_path,D.d_mode);
	}
	else	
		if (flag=='F')
		{
			strcat(strcat(F.f_path,New_Home_Path),"/");
			_recFile(&F);
			int file;
			if (!(file=open(F.f_path, (O_CREAT | O_RDWR), F.f_mode) ) )	//Create_Arch_File
				perror("Error Arch file");
			_dataFile(file,F.f_size);
			close(file);
		}
		else 
			{
				printf("ERROR:\n");
				return 1;
			}	
	int pos = lseek(File_arch,0,SEEK_CUR);
	if (lseek(File_arch, 0,SEEK_END)!=pos)
	{
		lseek(File_arch,pos,SEEK_SET);
		Unpacking();
	}
}
void _dataFile(const int file,unsigned size)
{
	char sym;
	for (unsigned i=0;i<size;i++)
	{
		read(File_arch,&sym,1);
		write(file,&sym,1);
	}
	lseek(File_arch,1,SEEK_CUR);
} 
void _recFile(struct _file *fl)
{
	lseek(File_arch, 1,SEEK_CUR);
	int count =0;
	char sym;
	char path[PATH_MAX];
	char str[64];
	char size[32];
	char mode[12];
	int n=0;
	while(read(File_arch, &sym,1))
	{
		if (sym=='\n')
			break;
		if (sym==';')
		{
			count++;
			n=0;
			continue;
		}
		if (count==0) 
			path[n++]=sym;
		if (count==1)
			str[n++]=sym;
		if (count==2)
			mode[n++]=sym;
		if (count==3)
			size[n++]=sym;
	}
	strcat(fl->f_path,path);
	strcpy(fl->f_name,str);
	strcat(strcat(fl->f_path,"/"),fl->f_name);
	fl->f_mode=atoi(mode);
	fl->f_size=atoi(size);
}
void _recDir(struct _dir * direct)
{
	lseek(File_arch, 1,SEEK_CUR);
	char sym;
	char toInt[16];
	int count = 0;
	int s=0;
	while (read(File_arch, &sym,1))
	{
		
		if (sym=='\n')
			break;
		if (sym==';')
			{
				count++;
				continue;
			}
		if (count==0)
			strcat(direct->d_path,&sym);
		else 
			toInt[s++]=sym;
	} 
	direct->d_mode = atoi(toInt);
}
char * Find_name (const char *_Adress)	
{
	return (strrchr(_Adress,'/'));
}
void Trent (DIR *where, char * directory, char * Path_kat)
{
	struct dirent *entry;
	_makePath(Path_kat);
	char _path[PATH_MAX];
	int _size;
	strcpy(_path, _makePath(Path_kat));
	_size = strlen(_path);
	
	_MDirWr(directory, _path);
	
	while ( (entry = readdir(where)) != NULL) 
	{
        if ( check_cond(entry->d_type,entry->d_name)=='c' )
        {
        	
        	char Path[PATH_MAX];
        	strcat( strcat(strcpy(Path,directory),"/"), entry->d_name);

        	Trent(opendir(Path), Path,strcat(Path_kat, Find_name(Path)));
        }
        if ( check_file(entry->d_type,entry->d_name)=='c' )
        {
        	write(File_arch,"F;",2);
        	write(File_arch, _path, _size);
        	char Path[PATH_MAX];
        	strcat( strcat(strcpy(Path,directory),"/"), entry->d_name);
        	int n = _MDwrite(entry, Path);	//Recording MetaData in Archive
        	_Dwrite(Path,n);
        	// организовать запись в архив метаданные Path; Time; Size byte \n {Data -> size byte}  write bug[BUF_SIZE]  
        }
    }
    if (closedir(where))
		perror("Error with close dir");
}
void _MDirWr(const char *_dir, const char *_path)
{
	struct stat _metaInfo;
	write(File_arch,"D;",2);
	write(File_arch,_path,strlen(_path));
	stat(_dir, & _metaInfo);
	char MT[32];
	int n = sprintf(MT,"%d",_metaInfo.st_mode);
	write(File_arch,MT,n);
	write(File_arch,"\n",1);
}
void _Dwrite(const char *_path, unsigned _size)
{
	char _buf[BUFSIZ],c;
	int _file = open(_path,O_RDWR);
	while(read(_file, &c, 1) == 1) 
		write(File_arch, &c, 1);
	write(File_arch,"\n",1);
	close(_file);	
}
int _MDwrite (const struct dirent * _file, const char * _path)
{
	struct stat _metaInfo;
	char MetaData[PATH_MAX];
	stat(_path, & _metaInfo);
	sprintf(MetaData,"%s%c%d%c%ld",_file->d_name,';', _metaInfo.st_mode,';' ,_metaInfo.st_size);
	write(File_arch, MetaData, strlen(MetaData));
	write(File_arch,"\n",1);
	return _metaInfo.st_size;
}
char * _makePath (const char * _path)
{
	char _buf[PATH_MAX];
	return strcat(strcpy(_buf, _path), ";");
} 
char check_cond (const int  _type,const  char * name)
{
	if ( ( _type ==4 ) && !( ( (name[0]=='.') && (strlen(name)==1) ) || ( (name[0]=='.') && (name[1]=='.') && (strlen(name)==2)  ) ) )
		return 'c';
	else 
		return 'a'; 
}
char check_file (const int  _type,const  char * name )
{
	if ( !( _type ==4 ) && !( ( (name[0]=='.') && (strlen(name)==1) ) || ( (name[0]=='.') && (name[1]=='.') && (strlen(name)==2)  ) ) )
		return 'c';
	else 
		return 'a'; 
}
