from ragger.backend import SpeculosBackend

def validate_displayed_message(client: SpeculosBackend, num_screen_skip: int):
    for _ in range(num_screen_skip):
        client.right_click()
    client.both_click()
