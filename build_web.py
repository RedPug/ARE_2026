"""
Takes the web assets (HTML, JavaScript, and CSS) and compiles them into a
single header file for embedding in the project.

Automatically run before compiling the project.
"""

import os
import minify_html

if 'Import' not in globals():
    def Import(name:str): pass
    env = {}

Import("env")

def compile_assets(env):
    html_path = os.path.join(env.get("PROJECT_DIR"), "web", "index.html")
    script_path = os.path.join(env.get("PROJECT_DIR"), "web", "script.js")
    style_path = os.path.join(env.get("PROJECT_DIR"), "web", "style.css")

    header_path = os.path.join(env.get("PROJECT_DIR"), "include", "index.hpp")

    with open(html_path, 'r', encoding='utf-8') as html_file:
        html_content = html_file.read()

    with open(script_path, 'r', encoding='utf-8') as script_file:
        script_content = script_file.read()

    with open(style_path, 'r', encoding='utf-8') as style_file:
        style_content = style_file.read()

    full_page = html_content.replace(
        '<script src="script.js"></script>', f'<script>{script_content}</script>'
        ).replace('<link rel="stylesheet" href="style.css">', f'<style>{style_content}</style>')

    minified_content = minify_html.minify(
        full_page,
        minify_js=True,                      # Condense inline JavaScript
        minify_css=True,                     # Condense inline CSS
        remove_processing_instructions=True, # Remove comments/processing instructions
        remove_bangs=True                    # Remove CDATA or optional declarations
    )

    with open(header_path, 'w', encoding='utf-8') as header_file:
        header_file.write("#pragma once\n")

        header_file.write('const char DASHBOARD_HTML[] PROGMEM = R"=====(\n')
        header_file.write(minified_content)
        header_file.write('\n)=====";')

        print("Web assets compiled and written to index.hpp")

compile_assets(env)