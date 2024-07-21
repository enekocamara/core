from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

import requests
import zipfile
import io
import os
import time

def read_extensions(file_path):
    """Read extensions from a file and return a list of extensions."""
    with open(file_path, 'r') as file:
        extensions = file.read().splitlines()
    return extensions

def construct_url(base_url, extensions):
    """Construct the URL with the specified extensions."""
    extensions_str = ','.join(extensions)
    # Replace the extensions part in the URL
    updated_url = base_url.split('&extensions=')[0] + f'&extensions={extensions_str}'
    return updated_url

def download_and_extract_zip(url, extract_to='.'):
    """Download and extract the ZIP file from the given URL."""
    response = requests.get(url)
    
    if response.status_code == 200:
        with io.BytesIO(response.content) as zip_file:
            with zipfile.ZipFile(zip_file, 'r') as zip_ref:
                zip_ref.extractall(extract_to)
                print(f"Extracted ZIP file to: {extract_to}")
    else:
        print(f"Failed to download the file. Status code: {response.status_code}")


# Get the directory of the script
script_dir = os.path.dirname(os.path.abspath(__file__))

# Base URL with placeholders for the extensions
base_url = "https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.6&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&extensions="
# Paths relative to the script directory
extensions_file_path = os.path.join(script_dir, 'extensions.txt')

# Read extensions from the file
extensions = read_extensions(extensions_file_path)

# Construct the URL with the extensions
url = construct_url(base_url, extensions)

driver = webdriver.Chrome()

# Navigate to the GLAD page
driver.get(url)

# Wait for the page to load
time.sleep(3)  # Adjust sleep time if needed

# Click the "Generate" button
# Locate the button by its class and type attributes
generate_button = driver.find_element(By.XPATH, '//section[@id="submit"]//input[@class="button" and @type="submit"]')
generate_button.click()

try:
    # Wait for the new page to load (or some expected change in the page)
    WebDriverWait(driver, 5).until(EC.url_changes(url))  # Create WebDriverWait instanc
    
    # Capture the current URL after redirection
    redirected_url = driver.current_url
    print(f"Redirected URL: {redirected_url}")

finally:
    driver.quit()
    # Download and extract the ZIP file
    download_and_extract_zip(redirected_url + 'glad.zip', script_dir)
