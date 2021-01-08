import undetected_chromedriver as uc
from chessWeb import util
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
import time
import chess


def init_game():
    chrome_options = Options()
    chrome_options.add_argument('--window-size=1920,1080')
    chrome_options.add_argument('--start-maximized')
    d = uc.Chrome(options=chrome_options)
    d.get('https://www.chess.com/play/computer')
    util.remove_popup(d)
    game = chess.Board()
    return d, game
    

def make_move(move_str, board, driver):
    move = util.parse_move(move_str, board)
    start_sq = chess.square_name(move.from_square)
    end_sq = chess.square_name(move.to_square)
    start_el = util.get_square_el(start_sq, driver)
    util.sleep_random(5, 10)
    util.click_el(start_el, driver)
    util.sleep_random(0.1, 1.5)
    end_el = util.get_square_el(end_sq, driver)
    util.click_el(end_el, driver)
    board.push(move)


def get_most_recent_move(board, driver):
    if board.turn == chess.WHITE:
        color = 'white'
    else:
        color = 'black'
    move_num = board.fullmove_number
    num_xpath = f"//div[@data-whole-move-number='{str(move_num)}']"
    class_name = color + ' node'
    move_xpath = num_xpath + f"//child::div[contains(@class,'{class_name}')]"

    util.wait_indefinite(num_xpath, driver)
    util.wait_indefinite(move_xpath, driver)

    move_el = driver.find_element_by_xpath(move_xpath)
    move_str = move_el.text
    board.push(board.parse_san(move_str))
    return move_str

