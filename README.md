# BEC - Cache Cowboys
Sand dune searcher with proper caching implementation

## Overview

Cache Cowboys is a demonstration of proper caching patterns and best practices. This project implements a cache manager that follows industry standards to avoid "cowboy coding" practices in cache management.

## Features

- **Cache-Aside Pattern**: Lazy loading with automatic cache population
- **TTL Support**: Time-to-live expiration for cached items
- **LRU Eviction**: Least Recently Used eviction when cache is full
- **Statistics Tracking**: Monitor cache hits, misses, and hit rates
- **Thread-Safe Operations**: Safe for concurrent access patterns

## Usage

### Basic Example

```python
from cache_cowboys import CacheManager, cached_search

# Initialize cache manager
cache = CacheManager(max_size=100, default_ttl=300)

# Use the cached_search decorator
@cached_search(cache)
def expensive_operation(query):
    # Your expensive operation here
    return fetch_data_from_database(query)

# First call fetches from source
result = expensive_operation("query")

# Second call uses cache
result = expensive_operation("query")  # Much faster!
```

### Running the Demo

```bash
python cache_cowboys.py
```

This will demonstrate the caching system with a sand dune search example.

### Running Tests

```bash
python -m unittest test_cache_cowboys.py
```

Or run with verbose output:

```bash
python -m unittest test_cache_cowboys.py -v
```

## Caching Best Practices

This implementation follows these best practices to avoid being a "cache cowboy":

1. **Clear Cache Invalidation**: TTL-based expiration prevents stale data
2. **Bounded Cache Size**: LRU eviction prevents unbounded memory growth
3. **Monitoring**: Built-in statistics for observability
4. **Documentation**: Clear documentation of caching behavior
5. **Testing**: Comprehensive test coverage for cache operations

## Requirements

- Python 3.6 or higher
- No external dependencies (uses standard library only)

## License

MIT License
