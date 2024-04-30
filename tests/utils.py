from pathlib import Path
from ragger.navigator import NavInsID

ROOT_SCREENSHOT_PATH = Path(__file__).parent.resolve()

def _navigation_helper(navigator, device_name: str, accept: bool, snapshots_name: str):
    if device_name.startswith("nano"):
        navigate_instruction = NavInsID.RIGHT_CLICK
        validation_instructions = [NavInsID.BOTH_CLICK]
        if accept:
            text = "Confirm"
        else:
            if device_name == "nanos":
                text = "Deny"
            else:
                text = "Reject"
    else:
        navigate_instruction = NavInsID.USE_CASE_REVIEW_TAP
        if accept:
            validation_instructions = [NavInsID.USE_CASE_REVIEW_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS]
            text = "Hold to sign"
        else:
            validation_instructions = [NavInsID.USE_CASE_REVIEW_REJECT, NavInsID.USE_CASE_CHOICE_CONFIRM, NavInsID.USE_CASE_STATUS_DISMISS]
            text = "Hold to sign"


    navigator.navigate_until_text_and_compare(navigate_instruction,
                                              validation_instructions,
                                              text,
                                              ROOT_SCREENSHOT_PATH,
                                              snapshots_name)

def navigation_helper_confirm(navigator, device_name: str, snapshots_name: str):
    _navigation_helper(navigator=navigator, device_name=device_name, accept=True, snapshots_name=snapshots_name)

def navigation_helper_reject(navigator, device_name: str, snapshots_name: str):
    _navigation_helper(navigator=navigator, device_name=device_name, accept=False, snapshots_name=snapshots_name)
