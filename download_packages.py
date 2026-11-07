"""
Downloads and installs the required Python packages for the project automatically.
"""

Import("env")

# Try to import the package; if missing, install it automatically
try:
    import minify_html
except ImportError:
    env.Execute("$PYTHONEXE -m pip install minify_html")
