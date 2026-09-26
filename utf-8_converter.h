#ifndef UTF_8_CONVERTER
#define UTF_8_CONVERTER 

typedef struct {
    char chars[4];
} utf_8_char;

typedef struct {
    char* const buf;
    const char* current;
    const size_t capacity;
    size_t size;
} reader_info;

#define MAX_WORD_LENGTH 128

short utf_8_size(char c);

utf_8_char read_utf_8_char(const char** c);

bool is_null_terminator(utf_8_char u);

bool are_equal(utf_8_char u_1, utf_8_char u_2);

bool load_word(const char* str, utf_8_char* word);

void print_utf_8_char(utf_8_char u);

reader_info new_reader_info(char* buf, size_t capacity);

bool read_utf_8_char_from_fd(int fd, reader_info* r, utf_8_char* out);

void print_word(utf_8_char* word);


#endif
