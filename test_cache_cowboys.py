#!/usr/bin/env python3
"""
Tests for Cache Cowboys implementation.
"""

import time
import unittest
from cache_cowboys import CacheManager, cached_search, search_sand_dunes


class TestCacheManager(unittest.TestCase):
    """Test cases for CacheManager class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.cache = CacheManager(max_size=5, default_ttl=2)
    
    def test_set_and_get(self):
        """Test basic set and get operations."""
        self.cache.set('key1', 'value1')
        self.assertEqual(self.cache.get('key1'), 'value1')
    
    def test_get_nonexistent_key(self):
        """Test getting a key that doesn't exist."""
        self.assertIsNone(self.cache.get('nonexistent'))
    
    def test_ttl_expiration(self):
        """Test that items expire after TTL."""
        self.cache.set('key1', 'value1', ttl=1)
        self.assertEqual(self.cache.get('key1'), 'value1')
        
        # Wait for expiration
        time.sleep(1.1)
        self.assertIsNone(self.cache.get('key1'))
    
    def test_lru_eviction(self):
        """Test that LRU eviction works when cache is full."""
        # Fill cache to capacity
        for i in range(5):
            self.cache.set(f'key{i}', f'value{i}')
        
        # Add one more item - should evict oldest
        self.cache.set('key5', 'value5')
        
        # key0 should be evicted
        self.assertIsNone(self.cache.get('key0'))
        self.assertEqual(self.cache.get('key5'), 'value5')
    
    def test_lru_update_on_access(self):
        """Test that accessing an item marks it as recently used."""
        # Fill cache
        for i in range(5):
            self.cache.set(f'key{i}', f'value{i}')
        
        # Access key0 to make it most recently used
        self.cache.get('key0')
        
        # Add new item - should evict key1 (now oldest)
        self.cache.set('key5', 'value5')
        
        # key0 should still exist, key1 should be evicted
        self.assertEqual(self.cache.get('key0'), 'value0')
        self.assertIsNone(self.cache.get('key1'))
    
    def test_delete(self):
        """Test deleting cache entries."""
        self.cache.set('key1', 'value1')
        self.cache.delete('key1')
        self.assertIsNone(self.cache.get('key1'))
    
    def test_clear(self):
        """Test clearing all cache entries."""
        self.cache.set('key1', 'value1')
        self.cache.set('key2', 'value2')
        self.cache.clear()
        
        self.assertIsNone(self.cache.get('key1'))
        self.assertIsNone(self.cache.get('key2'))
        self.assertEqual(self.cache.get_stats()['size'], 0)
    
    def test_stats_tracking(self):
        """Test that cache statistics are tracked correctly."""
        self.cache.set('key1', 'value1')
        
        # Generate hits and misses
        self.cache.get('key1')  # hit
        self.cache.get('key1')  # hit
        self.cache.get('key2')  # miss
        
        stats = self.cache.get_stats()
        self.assertEqual(stats['hits'], 2)
        self.assertEqual(stats['misses'], 1)
        self.assertEqual(stats['total_requests'], 3)
        self.assertEqual(stats['size'], 1)
    
    def test_update_existing_key(self):
        """Test updating an existing key."""
        self.cache.set('key1', 'value1')
        self.cache.set('key1', 'value2')
        
        self.assertEqual(self.cache.get('key1'), 'value2')
        # Size should still be 1
        self.assertEqual(self.cache.get_stats()['size'], 1)


class TestCachedSearch(unittest.TestCase):
    """Test cases for cached search functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.cache = CacheManager(max_size=10, default_ttl=60)
        self.call_count = 0
    
    def mock_expensive_operation(self, query: str) -> str:
        """Mock expensive operation that increments call count."""
        self.call_count += 1
        return f"result for {query}"
    
    def test_cached_search_decorator(self):
        """Test that the cached_search decorator works correctly."""
        @cached_search(self.cache)
        def cached_func(query: str) -> str:
            return self.mock_expensive_operation(query)
        
        # First call should execute function
        result1 = cached_func('test')
        self.assertEqual(self.call_count, 1)
        self.assertEqual(result1, 'result for test')
        
        # Second call should use cache
        result2 = cached_func('test')
        self.assertEqual(self.call_count, 1)  # Should not increment
        self.assertEqual(result2, 'result for test')
        
        # Different query should execute function again
        result3 = cached_func('other')
        self.assertEqual(self.call_count, 2)
        self.assertEqual(result3, 'result for other')


class TestSandDuneSearch(unittest.TestCase):
    """Test cases for sand dune search functionality."""
    
    def test_search_sand_dunes(self):
        """Test sand dune search returns expected results."""
        result = search_sand_dunes('sahara')
        
        self.assertIn('query', result)
        self.assertIn('results', result)
        self.assertIn('count', result)
        self.assertEqual(result['query'], 'sahara')
        self.assertGreater(result['count'], 0)
        self.assertIsInstance(result['results'], list)
    
    def test_search_sand_dunes_no_results(self):
        """Test sand dune search with no matches."""
        result = search_sand_dunes('antarctica')
        
        self.assertEqual(result['count'], 0)
        self.assertEqual(result['results'], [])


if __name__ == '__main__':
    unittest.main()
