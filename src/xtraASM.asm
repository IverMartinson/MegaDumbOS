section .text
global xtraASM, writeToDisk, readFromDisk, enableInterrupts, protectedToReal, realToProtected

xtraASM:
    ret

writeToDisk:
    ret

enableInterrupts:
    ret

protectedToReal:
    ret

realToProtected:
    ret

readFromDisk:
    ret

section .data
    LBA dd 100
    ;dataBuffer dw 256 dup(0) ; allocate space for 256 words in dataBuffer
