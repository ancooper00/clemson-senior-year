#!/usr/bin/python3
# coding: latin-1
blob = """
                �����S>�a�h
�rZ4����L&9%'0�ܱ��~d&Gdړԓ�	@C�4�,���r�H���D�T�Ss9!%�9�*�[C�2N�O���K_������	{~��oQҹ%�����rLS��2F
"""
import urllib.parse
q = urllib.parse.quote(blob)
if len(q) == 445:
    print("Use SHA-256 instead!")
else:
    print("MD5 is perfectly secure!")
