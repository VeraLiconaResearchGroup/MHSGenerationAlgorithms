import inspect

def __line__():
    caller = inspect.stack()[1]
    return int (caller[2])

def __function__():
    caller = inspect.stack()[1]
    return caller[3]