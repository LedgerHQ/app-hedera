from pathlib import Path
from ragger.firmware import Firmware
from ragger.navigator import NavInsID


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


def navigate_erc20_confirm(firmware, navigator, scenario_navigator, screenshots_path: Path, test_name: str):
    """Standard ERC20 navigation schema: capture NBGL warning, then approve review."""
    if firmware.is_nano:
        scenario_navigator.review_approve()
    else:
        navigator.navigate_and_compare(
            screenshots_path,
            f"{test_name}_warning",
            [NavInsID.USE_CASE_CHOICE_CONFIRM],
            screen_change_after_last_instruction=False,
        )
        scenario_navigator.review_approve(test_name=test_name)


def navigate_erc20_reject(firmware, navigator, scenario_navigator, screenshots_path: Path, test_name: str):
    """Standard ERC20 navigation schema: capture NBGL warning, then reject review."""
    if firmware.is_nano:
        scenario_navigator.review_reject()
    else:
        navigator.navigate_and_compare(
            screenshots_path,
            f"{test_name}_warning",
            [NavInsID.USE_CASE_CHOICE_CONFIRM],
            screen_change_after_last_instruction=False,
        )
        scenario_navigator.review_reject(test_name=test_name)

def navigate_erc20_reject_at_warning(firmware, navigator, scenario_navigator, screenshots_path: Path, test_name: str):
    """Standard ERC20 navigation schema: capture NBGL warning, then reject review."""
    if firmware.is_nano:
        scenario_navigator.review_reject()
    else:
        navigator.navigate_and_compare(
            screenshots_path,
            f"{test_name}_warning",
            [NavInsID.USE_CASE_CHOICE_REJECT],
            screen_change_after_last_instruction=True,
        )