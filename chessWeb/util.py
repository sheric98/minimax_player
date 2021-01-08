import math
import random
import time
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support import expected_conditions as EC

def sleep_random(start, end):
    sleep_time = random.uniform(start, end)
    time.sleep(sleep_time)


def remove_popup(driver):
    class_name = 'icon-font-chess x modal-seo-close-icon'
    el = WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, f"//span[@class='{class_name}']")))
    x = driver.find_element_by_xpath(f"//span[@class='{class_name}']")
    x.click()


def wait_indefinite(xpath, driver):
    while True:
        try:
            WebDriverWait(driver, 30).until(EC.presence_of_element_located((By.XPATH, xpath)))
            break
        except TimeoutError:
            continue


def get_offset(dim):
    usable = (math.floor(0.9 * dim) - 1) // 2
    if usable <= 0:
        return 0
    return random.randint(-usable, usable)


def click_el(el, driver):
    off_x = get_offset(el.size['width'])
    off_y = get_offset(el.size['height'])
    ac = ActionChains(driver)
    ac.move_to_element(el).move_by_offset(off_x, off_y).click().perform()


def get_square_el(square_str, driver):
    class_num = str(file_num(square_str[0])) + square_str[1]
    class_name = f'square-{class_num}'
    xpath = f"//div[contains(@class,'{class_name}')]"
    return driver.find_element_by_xpath(xpath)
    

def parse_move(move_str, board):
    try:
        move = board.parse_san(move_str)
        return move
    except ValueError:
        try:
            move = board.parse_uci(move_str)
            return move
        except ValueError:
            print('Illegal or uninterpretable move')
            exit(-1)

def file_num(x):
    return ord(x) - ord('a') + 1