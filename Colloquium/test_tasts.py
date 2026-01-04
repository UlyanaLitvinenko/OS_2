import unittest
from io import StringIO
import sys

from your_module import fibonacci, is_palindrome, reverse_list

class TestTasks(unittest.TestCase):

    def test_fibonacci(self):
        self.assertEqual(fibonacci(0), [])
        self.assertEqual(fibonacci(1), [1])
        self.assertEqual(fibonacci(5), [1, 1, 2, 3, 5])
        self.assertEqual(fibonacci(10), [1, 1, 2, 3, 5, 8, 13, 21, 34, 55])

    def test_is_palindrome(self):
        self.assertTrue(is_palindrome(121))
        self.assertTrue(is_palindrome(1221))
        self.assertTrue(is_palindrome(1))
        self.assertFalse(is_palindrome(123))
        self.assertFalse(is_palindrome(10))

    def test_reverse_list(self):
        self.assertEqual(reverse_list([]), [])
        self.assertEqual(reverse_list([1]), [1])
        self.assertEqual(reverse_list([1, 2, 3]), [3, 2, 1])
        self.assertEqual(reverse_list([5, 4, 3, 2]), [2, 3, 4, 5])

if __name__ == '__main__':
    unittest.main()
