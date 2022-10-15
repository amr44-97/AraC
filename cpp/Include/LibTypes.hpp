#ifndef AM_LIBTYPES_H
#define AM_LIBTYPES_H

#include <string.h>
#include <string>
#include <stdint.h>
#include <stddef.h>
#include <typeinfo>
#include <stdlib.h>

#define null NULL


typedef uint8_t                u8;
typedef int8_t                 i8;

typedef uint16_t               u16;
typedef int16_t                i16;

typedef uint32_t               u32;
typedef int32_t                i32;

typedef uint64_t               u64;
typedef int64_t                i64;
typedef size_t                 usize;


typedef unsigned char* cstr;

struct String;
//typedef struct List List;


template<typename T>
struct List;



void StringFreeAll();

String StringBuild(const char *__str);


struct String {
   size_t length;
   size_t capacity;
   char*  str;


    private:
    void add_strptr_stack();
    void update_ptr_pointer(void *old_ptr, void *new_ptr);
   // int check_marked_free(String __str);
   // int check_marked_free_ptr(char *__str);




    public:  
    void    cat_m(const char * __char);
    void    cat(const char * __char); 
    String  cat_to_new(const char * __char); 
    void    push(char c);
    void    input();
    List<char*>   split();
    List<char*>   split_by_delim(const char delimeter[]);
    String  substr(size_t st_pos, size_t n);


    void destroy();




constexpr int 
find_char(char element){
  for (int i = 0; i < (int) this->length; i++) {
    if (this->str[i] == element) {
      return i;
    }
  }
  return 0;

}

constexpr int 
find_char(size_t cur_index,char element){
      for (int i = cur_index; i < (int) this->length; i++) {
    if (this->str[i] == element) {
      return i;
      break;
    }
  }
  return 0;

}


    String  Build (const char *__str); 
    
    String  Build_s(const char *__str, size_t size);


    
    void operator +=(const char* s){ this->cat(s); }
 
    void operator +=(String s){ this->cat(s.str);}

    char operator[](int index){
        return this->str[index];
    }

    String operator+(const char* s){ return cat_to_new(s); }
    
    String operator+(String s){ return cat_to_new(s.str); }
    


};



void   _unused__cat_List_fn(String *__str, ...);
#define Str_cat_List(str,...) _unused__cat_List_fn(str,__VA_ARGS__,NULL)


//void Str_cat_m(String *__str, const char *__char) ;

//void Str_cat(String *__str, const char *__char) ;


template<typename T>
struct List {
    public:
    T* ptr  = nullptr;
    size_t length   = 0;
    size_t capacity = 0;
    
    private:
    
    void extend(size_t num){
        this->ptr = (T*)realloc(this->ptr, ((num+ this->capacity) * sizeof(T)) );
        this->capacity += num;
    }

    public:

  constexpr  void push_back(T element){
        size_t len = this->length;
        this->extend(1);
        this->ptr[len] = element;
        this->length+=1;
    }

   constexpr void pop_back(){
       // size_t len = this->length;
       // this->ptr[len] = nullptr;
        this->length-=1;
    }

    
    T operator[](int i){
            return this->ptr[i];
    }


    void destroy(){
        free(this->ptr);
    }

    
};


namespace fmt{
    List<String> Parse(String fmt);
    
        void Println(int x);
        void Println(float x);
        void Println(int x);
        void Println(unsigned int x);
        void Println(size_t x);
        void Println(String x);
        void Println(char x);
        void Println(List<char*> x);
        void Println(List<char> x);
        void Println(List<String> x);
        void Println(const char* x);
        void Println(std::string x);
        void Println(int* x);

};



#endif // AM_LIBTYPES_H
