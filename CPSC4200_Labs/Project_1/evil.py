#!/usr/bin/python3
# coding: latin-1
blob = """
                �����S>�a�h
�rZ4�����L&9%'0�ܱ��~d&Gdړ��	@C�4�,����H���D�T�Ss9!%�9�*�[C�N�O���K_������	{~��oQ�9%�����rL���2F
"""
import urllib.parse
q = urllib.parse.quote(blob)
if len(q) == 445:
    print("Use SHA-256 instead!")
else:
    print("MD5 is perfectly secure!")
