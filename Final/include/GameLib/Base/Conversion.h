#ifndef INCLUDED_GAMELIB_CONVERSION_H
#define INCLUDED_GAMELIB_CONVERSION_H


//文字列への変換
//charなら最大4文字、shortなら6文字、intなら11文字、floatなら16、doubleなら30前後(予定)の
//サイズのバッファを渡す必要がある。
//戻り値は書き込みサイズだ。範囲外にあふれていないかチェックしよう。
namespace GameLib{

int toString10( char* out, char ); //文字でなく単なる整数として扱う
int toString10( char* out, unsigned char );
int toString10( char* out, short );
int toString10( char* out, unsigned short );
int toString10( char* out, int );
int toString10( char* out, unsigned );

int toString16( char* out, char ); //文字でなく単なる整数として扱う
int toString16( char* out, unsigned char );
int toString16( char* out, short );
int toString16( char* out, unsigned short );
int toString16( char* out, int );
int toString16( char* out, unsigned );

int toString( char* out, float, int precision = 6 );
int toString( char* out, double, int precision = 6 );

} //namespace GameLib

#endif