#ifndef RC4_H
#define RC4_H

#include <QObject>

class RC4
{
public:
    RC4();
    void init(const unsigned char* key, unsigned long len);
    void crypt(unsigned char* in, unsigned char* out, unsigned long len);
    void clear();

private:
    int m_box[256];
    int m_index_i;
    int m_index_j;
};

#endif // RC4_H
