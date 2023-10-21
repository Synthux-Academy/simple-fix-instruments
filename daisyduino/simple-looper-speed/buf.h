#pragma once

namespace synthux {

class Buffer {
public:
  float Read(size_t frame) {
    if (frame >= _buf_len) frame -= _buf_len;
    return _buf[frame];
  }


private:
  float *_buf;
  size_t _buf_len;
};

};