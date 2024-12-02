#!/usr/bin/env python3

import os
import sys
import subprocess
import argparse
from datetime import datetime

def analyze_cpu_profile(profile_path):
    """Анализ CPU профиля"""
    print(f"\nAnalyzing CPU profile: {profile_path}")
    
    # Топ горячих функций
    subprocess.run([
        "pprof", "--top", "--cum",
        profile_path
    ])
    
    # Граф вызовов
    subprocess.run([
        "pprof", "--pdf",
        "--focus=main",
        profile_path,
        f"{profile_path}.pdf"
    ])

def analyze_heap_profile(profile_path):
    """Анализ профиля памяти"""
    print(f"\nAnalyzing heap profile: {profile_path}")
    
    # Топ аллокаций
    subprocess.run([
        "pprof", "--top", "--inuse_space",
        profile_path
    ])
    
    # Утечки памяти
    subprocess.run([
        "pprof", "--list=.",
        "--focus=leaked",
        profile_path
    ])

def main():
    parser = argparse.ArgumentParser(description='Analyze performance profiles')
    parser.add_argument('profile_dir', help='Directory with profile files')
    parser.add_argument('--type', choices=['cpu', 'heap', 'all'],
                       default='all', help='Type of profiles to analyze')
    args = parser.parse_args()

    for file in os.listdir(args.profile_dir):
        if args.type in ['cpu', 'all'] and file.startswith('cpu_profile_'):
            analyze_cpu_profile(os.path.join(args.profile_dir, file))
        
        if args.type in ['heap', 'all'] and file.startswith('heap_profile_'):
            analyze_heap_profile(os.path.join(args.profile_dir, file))

if __name__ == '__main__':
    main() 