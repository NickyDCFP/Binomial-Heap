# Binomial Heap

A binomial heap that supports O(1) am. insertion and O(log n) merging, matching binary heaps for the time complexities of all other operations. 

| Operation | Binary Heaps | Binomial Heaps |
| --------- | ------------ | -------------- |
| Report Min | $$O(1)$$ | $$O(1)$$ |
| Extract Min | $$O(log n)$$ | $$O(log n)$$ |
| Insert | $$O(log n)$$ | $${\color{green}O(1) am.}$$ |
| Decrease Key | $$O(log n)$$ | $$O(log n)$$ |
| Delete | $$O(log n)$$ | $$O(log n)$$ |
| Merge | $$O(n)$$ | $${\color{green}O(log n)}$$ |

As a side note, binary heaps can also achieve O(1) am. for n inserts. That is, if n elements are being inserted into the heap at once, they can be appended onto the end, and the heap can be rebuilt for a total of linear time.
