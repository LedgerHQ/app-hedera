from pathlib import Path
from ragger.firmware import Firmware


ROOT_SCREENSHOT_PATH = Path(__file__).parent.resolve()

def navigation_helper_confirm(firmware, scenario_navigator):
    if firmware.is_nano:
        scenario_navigator.review_approve(custom_screen_text="Confirm")
    else:
        scenario_navigator.review_approve()

def navigation_helper_reject(firmware, scenario_navigator):
    if firmware is Firmware.NANOS:
        scenario_navigator.review_reject(custom_screen_text="Deny")
    elif firmware.is_nano:
        scenario_navigator.review_reject(custom_screen_text="Reject")
    else:
        scenario_navigator.review_reject()