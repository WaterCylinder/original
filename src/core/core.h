#ifndef CORE_H
#define CORE_H

/**
 * @file core.h
 * @brief Core module header file
 * @details Contains all fundamental components that form the core of the Original project.
 * Integrates the following main functional modules:
 *
 * @section Main_Features Main Features
 *
 * @subsection Containers
 * - Fixed-size containers: array, bitSet
 * - Variable-size containers: vector, forwardChain, chain, blocksList
 *
 * @subsection Memory_Management
 * - Smart pointers: ownerPtr, strongPtr, weakPtr
 * - Allocators: allocatorBase, allocator, objPoolAllocator
 * - Deleters: deleterBase, deleter
 *
 * @subsection Algorithms_Iterators
 * - Algorithms: allOf/anyOf, find/count, sort/stableSort etc.
 * - Iterators: iterator, iterAdaptor, directional iterators
 * - Algorithm adapters: transform, filter, comparator
 *
 * @subsection Utilities
 * - Interfaces: printable, comparable, cloneable
 * - Tuples: couple, tuple
 * - Others: error handling, maths utilities, config
 *
 * @section Usage
 * Include this single header to access all core functionality:
 * @code
 * #include "original/core.h"
 * @endcode
 *
 * @note Full documentation available at: https://documents-original.vercel.app/
 * @see README.md for project overview and build instructions
 * @see allocator.h For detailed memory management documentation
 * @see chain.h For linked list implementation details
 */


#include "algorithms.h"
#include "allocator.h"
#include "array.h"
#include "autoPtr.h"
#include "baseArray.h"
#include "baseList.h"
#include "bitSet.h"
#include "blocksList.h"
#include "chain.h"
#include "cloneable.h"
#include "comparable.h"
#include "comparator.h"
#include "config.h"
#include "container.h"
#include "containerAdapter.h"
#include "couple.h"
#include "deleter.h"
#include "deque.h"
#include "doubleDirectionIterator.h"
#include "error.h"
#include "filter.h"
#include "filterStream.h"
#include "forwardChain.h"
#include "hash.h"
#include "iterable.h"
#include "iterationStream.h"
#include "iterator.h"
#include "map.h"
#include "maps.h"
#include "maths.h"
#include "optional.h"
#include "ownerPtr.h"
#include "printable.h"
#include "prique.h"
#include "queue.h"
#include "randomAccessIterator.h"
#include "RBTree.h"
#include "refCntPtr.h"
#include "serial.h"
#include "set.h"
#include "singleDirectionIterator.h"
#include "skipList.h"
#include "stack.h"
#include "stepIterator.h"
#include "transform.h"
#include "transformStream.h"
#include "tuple.h"
#include "types.h"
#include "vector.h"
#include "wrapper.h"

#endif //CORE_H
