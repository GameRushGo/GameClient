#include "rc4.h"

RC4::RC4()
{
    m_index_i = 0;
    m_index_j = 0;
}

void RC4::init(const unsigned char *key, unsigned long len)
{
    if (key == NULL || len == 0) {
        printf("rc4 key or len is 0, return! ");
        return ;
    }

    // for循环将0到255的互不重复的元素装入S盒
    for (int i = 0; i < 256 ; i++) {
        m_box[i] = i;
    }

    // for循环根据密钥打乱S盒
    int j = 0;
    unsigned char tmp;
    for (int i = 0; i < 256; i++) {
        j = ( j + m_box[i] + key[i % len] ) % 256;

        tmp = m_box[i];
        m_box[i] = m_box[j]; //交换m_box[i]和m_box[j]
        m_box[j] = tmp;
    }
}


void RC4::crypt(unsigned char* in, unsigned char* out, unsigned long len)
{
    // 每收到一个字节，就进行while循环。通过一定的算法（(a),(b)）定位S盒中的一个元素，并与输入字节异或，得到k。循环中还改变了S盒（(c)）。如果输入的是明文，输出的就是密文；如果输入的是密文，输出的就是明文。
    unsigned char tmp;
    for(unsigned long k = 0 ; k < len ; k++){
        m_index_i = (m_index_i + 1) % 256;    // a
        m_index_j = (m_index_j + m_box[m_index_i] ) % 256; // b

        tmp = m_box[m_index_i];
        m_box[m_index_i] = m_box[m_index_j]; //交换m_box[x]和m_box[y]
        m_box[m_index_j] = tmp;

        //生成伪随机数
        int r = ( m_box[m_index_i] + m_box[m_index_j] ) % 256;
        out[k] = in[k] ^ m_box[r];        
    }
}

void RC4::clear()
{
    m_index_i = 0;
    m_index_j = 0;
    memset(m_box, 0, 256);
}
