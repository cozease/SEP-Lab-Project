import argparse
from difflib import Differ
import os

def diff_check(student_file:str, raw_file: str="./raw.txt"):
    
    differ = Differ()
    with open(student_file, 'r', encoding='utf-8')  as fs, open(raw_file, 'r', encoding='utf-8') as fr:
        student_content, raw_content = fs.readlines(), fr.readlines()
    student_content = [line.replace('\r\n', '\n') for line in student_content]
    raw_content = [line.replace('\r\n', '\n') for line in raw_content]
    diff = list(differ.compare(student_content, raw_content))
    
    cot = 0
    for diff_line in diff:
        if diff_line[0] in ['+', '-']:
            cot += 1

    print(f"{cot}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-r', '--raw', type=str, help="Path of raw file", required=True)
    parser.add_argument('-s', '--stu', type=str, help="Path of students file", required=True)

    args = parser.parse_args()
        
    if not os.path.exists(args.raw):
        print(f"raw file not found: {args.raw}")
        exit(1)
    if not os.path.exists(args.stu):
        print(f"student file not found: {args.stu}")
        exit(1)

    diff_check(student_file=args.stu, raw_file=args.raw)

main()