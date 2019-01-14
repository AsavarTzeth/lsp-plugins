/*
 * Allocator3D.cpp
 *
 *  Created on: 14 янв. 2019 г.
 *      Author: sadko
 */

#include <core/3d/Allocator3D.h>

namespace lsp
{
    BasicAllocator3D::BasicAllocator3D(size_t sz_of, size_t c_size)
    {
        nCapacity       = 0;
        nChunks         = 0;
        nChunkCapacity  = c_size;
        nChunkSize      = c_size;
        nSizeOf         = sz_of;
        pCurrChunk      = NULL;
        vChunks         = NULL;
    }

    BasicAllocator3D::~BasicAllocator3D()
    {
        do_destroy();
    }

    void BasicAllocator3D::allocate_new_chunk()
    {
        // Try to allocate chunk data
        uint8_t *chunk = reinterpret_cast<uint8_t *>(::malloc(nChunkCapacity * nSizeOf));
        if (chunk == NULL)
            return;

        // Register chunk data
        if (nChunks >= nCapacity)
        {
            uint8_t **nc    = reinterpret_cast<uint8_t **>(::realloc(vChunks, sizeof(uint8_t *) * (nCapacity + 16)));
            if (nc == NULL)
            {
                free(chunk);
                return;
            }
            vChunks         = nc;
            nCapacity      += 16;
        }

        // Store chunk and reset it's size
        vChunks[nChunks++]  = chunk;
        pCurrChunk          = chunk;
        nChunkSize          = 0;
    }

    void *BasicAllocator3D::do_alloc()
    {
        if (nChunkSize >= nChunkCapacity)
        {
            allocate_new_chunk();
            if (nChunkSize >= nChunkCapacity)
                return NULL;
        }
        return &pCurrChunk[nSizeOf * (nChunkSize++)];
    }

    void BasicAllocator3D::do_destroy()
    {
        if (vChunks != NULL)
        {
            for (size_t i=0; i<nChunks; ++i)
            {
                uint8_t *c = vChunks[i];
                if (c != NULL)
                {
                    ::free(c);
                    vChunks[i] = NULL;
                }
            }

            ::free(vChunks);
            vChunks = NULL;
        }

        nCapacity       = 0;
        nChunks         = 0;
        pCurrChunk      = NULL;
    }
}
