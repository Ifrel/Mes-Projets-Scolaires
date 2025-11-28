#include "io_fun.h"
#include "status.h"

ERR_CODE set_u32(FILE *dest, uint32_t buffer)
{
    uint8_t bytes[4];

    // Arrange bytes in big-endian order (MSB first)
    bytes[0] = (buffer >> 24) & 0xFF;
    bytes[1] = (buffer >> 16) & 0xFF;
    bytes[2] = (buffer >> 8) & 0xFF;
    bytes[3] = buffer & 0xFF;

    // Write all 4 bytes to the file
    if (fwrite(bytes, 1, 4, dest) != 4)
    {
        return ERR_WRITE; // Error occurred
    }
    return OK;
}

ERR_CODE set_u16(FILE *dest, uint16_t buffer)
{
    uint8_t bytes[2];
    bytes[0] = (buffer >> 8) & 0xFF; // MSB
    bytes[1] = buffer & 0xFF;        // LSB

    if (fwrite(bytes, 1, 2, dest) != 2)
    {
        return ERR_WRITE; // Error occurred
    }
    return OK;
}

ERR_CODE set_u8(FILE *dest, uint8_t buffer)
{
    if (fwrite(&buffer, 1, 1, dest) != 1)
    {
        return ERR_WRITE; // Error occurred
    }

    return OK;
}

ERR_CODE get_u32(FILE *stream, uint32_t *buffer)
{
    uint32_t result = 0;
    for (size_t i = 0; i < 4; i++)
    {
        int octet = fgetc(stream);
        if (octet == EOF)
        {
            /*perror("Erreur lors de la lecture d'un octet");*/
            return ERR_READ;
        }
        result = (result << 8) | (uint8_t)octet; // Décale et ajoute l'octet lu
    }
    *buffer = result;
    return OK;
}

ERR_CODE get_u16(FILE *stream, uint16_t *buffer)
{
    uint16_t result = 0;
    for (size_t i = 0; i < 2; i++)
    {
        int octet = fgetc(stream);
        if (octet == EOF)
        {
            /*perror("Erreur lors de la lecture d'un octet");*/
            return ERR_READ;
        }
        result = (result << 8) | (uint8_t)octet; // Décale et ajoute l'octet lu
    }
    *buffer = result;
    return OK;
}

ERR_CODE get_u8(FILE *stream, uint8_t *buffer)
{
    int octet = fgetc(stream);
    if (octet == EOF)
    {
        /*perror("Erreur lors de la lecture d'un octet");*/
        return ERR_READ;
    }

    *buffer = octet;
    return OK;
}
