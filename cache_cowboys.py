#!/usr/bin/env python3
"""
Cache Cowboys - A demonstration of proper caching patterns

This module implements a cache manager with common caching strategies
to avoid "cowboy coding" practices in cache management.
"""

import time
from functools import lru_cache
from typing import Any, Optional, Callable
from collections import OrderedDict


class CacheManager:
    """
    A simple cache manager implementing cache-aside pattern with TTL support.
    
    Cache-aside (lazy loading) is a common caching pattern where:
    1. Application checks cache first
    2. If miss, fetch from source
    3. Store result in cache
    4. Return result
    """
    
    def __init__(self, max_size: int = 100, default_ttl: int = 300):
        """
        Initialize the cache manager.
        
        Args:
            max_size: Maximum number of items to store in cache
            default_ttl: Default time-to-live in seconds for cached items
        """
        self.max_size = max_size
        self.default_ttl = default_ttl
        self._cache = OrderedDict()
        self._timestamps = {}
        self._hits = 0
        self._misses = 0
    
    def get(self, key: str) -> Optional[Any]:
        """
        Get value from cache if it exists and is not expired.
        
        Args:
            key: Cache key
            
        Returns:
            Cached value or None if not found or expired
        """
        if key not in self._cache:
            self._misses += 1
            return None
        
        # Check if expired
        if self._is_expired(key):
            self.delete(key)
            self._misses += 1
            return None
        
        # Move to end to mark as recently used (LRU)
        self._cache.move_to_end(key)
        self._hits += 1
        return self._cache[key]
    
    def set(self, key: str, value: Any, ttl: Optional[int] = None) -> None:
        """
        Set value in cache with optional TTL.
        
        Args:
            key: Cache key
            value: Value to cache
            ttl: Time-to-live in seconds (uses default_ttl if not specified)
        """
        # Evict oldest item if at capacity
        if key not in self._cache and len(self._cache) >= self.max_size:
            self._cache.popitem(last=False)
        
        self._cache[key] = value
        self._cache.move_to_end(key)
        self._timestamps[key] = time.time() + (ttl or self.default_ttl)
    
    def delete(self, key: str) -> None:
        """
        Delete key from cache.
        
        Args:
            key: Cache key to delete
        """
        if key in self._cache:
            del self._cache[key]
            del self._timestamps[key]
    
    def clear(self) -> None:
        """Clear all cache entries."""
        self._cache.clear()
        self._timestamps.clear()
        self._hits = 0
        self._misses = 0
    
    def _is_expired(self, key: str) -> bool:
        """Check if cache entry has expired."""
        return time.time() > self._timestamps.get(key, 0)
    
    def get_stats(self) -> dict:
        """
        Get cache statistics.
        
        Returns:
            Dictionary with cache stats including hits, misses, and hit rate
        """
        total = self._hits + self._misses
        hit_rate = (self._hits / total * 100) if total > 0 else 0
        
        return {
            'hits': self._hits,
            'misses': self._misses,
            'total_requests': total,
            'hit_rate': f"{hit_rate:.2f}%",
            'size': len(self._cache),
            'max_size': self.max_size
        }


def cached_search(cache_manager: CacheManager) -> Callable:
    """
    Decorator for caching search results.
    
    Args:
        cache_manager: CacheManager instance to use
        
    Returns:
        Decorator function
    """
    def decorator(func: Callable) -> Callable:
        def wrapper(query: str) -> Any:
            # Check cache first
            cached_result = cache_manager.get(query)
            if cached_result is not None:
                print(f"Cache HIT for query: {query}")
                return cached_result
            
            # Cache miss - fetch from source
            print(f"Cache MISS for query: {query}")
            result = func(query)
            
            # Store in cache
            cache_manager.set(query, result)
            return result
        
        return wrapper
    return decorator


# Example: Sand dune search function
def search_sand_dunes(query: str) -> dict:
    """
    Simulates an expensive search operation.
    
    Args:
        query: Search query
        
    Returns:
        Search results
    """
    # Simulate expensive operation
    time.sleep(0.5)
    
    # Mock search results
    dunes = {
        'sahara': ['Great Sand Sea', 'Erg Chech', 'Grand Erg Oriental'],
        'arabian': ['Rub al Khali', 'An Nafud', 'Ad Dahna'],
        'gobi': ['Khongoryn Els', 'Gobi Gurvansaikhan'],
        'namib': ['Sossusvlei', 'Sandwich Harbour']
    }
    
    results = []
    for region, dune_list in dunes.items():
        if query.lower() in region.lower():
            results.extend(dune_list)
    
    return {
        'query': query,
        'results': results,
        'count': len(results),
        'timestamp': time.time()
    }


def main():
    """Main demonstration of cache usage."""
    print("=== Cache Cowboys - Proper Caching Demo ===\n")
    
    # Initialize cache manager
    cache = CacheManager(max_size=10, default_ttl=60)
    
    # Create cached version of search function
    @cached_search(cache)
    def cached_dune_search(query: str) -> dict:
        return search_sand_dunes(query)
    
    # Demonstrate cache usage
    queries = ['sahara', 'arabian', 'sahara', 'gobi', 'sahara', 'namib']
    
    print("Running searches...")
    for query in queries:
        result = cached_dune_search(query)
        print(f"  Found {result['count']} dunes for '{query}'\n")
    
    # Show cache statistics
    print("\nCache Statistics:")
    stats = cache.get_stats()
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    print("\n=== Demo Complete ===")


if __name__ == '__main__':
    main()
