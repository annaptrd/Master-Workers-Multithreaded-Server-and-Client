
#ifndef OUTPUTPARAMETERS_H
#define OUTPUTPARAMETERS_H

class OutputParameters {
public:
    int r_fd;
    int s_fd;
    int bufferSize;
    
    OutputParameters(int r_fd, int s_fd, int bufferSize) :
    r_fd(r_fd), s_fd(s_fd), bufferSize(bufferSize) {
    }

};

#endif /* OUTPUTPARAMETERS_H */

