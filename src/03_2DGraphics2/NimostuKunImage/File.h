#ifndef INCLUDED_FILE_H
#define INCLUDED_FILE_H

class File{
public:
   File( const char* filename );
   ~File();
   int getSize() const;
   const char* getData() const;
   unsigned getUnsigned( int position ) const;
private:
   int mSize;
   char* mData;
};

#endif
