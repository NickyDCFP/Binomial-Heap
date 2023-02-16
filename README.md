# Binomial Heap

A binomial heap that supports O(1) am. insertion and O(log n) merging, matching binary heaps for the time complexities of all other operations. 

| Operation | Binary Heaps | Binomial Heaps |
| --------- | ------------ | -------------- |
| Report Min | O(1) | O(1) |
| Extract Min | O(log n) | O(log n) |
| Insert | O(log n), O(1) am. for n inserts | O(1) am. |
| Decrease Key | O(log n) | O(log n) |
| Delete | O(log n) | O(log n) |
| Merge | O(n) | O(log n) |