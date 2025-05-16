from .core import *
from threading import Thread


class CommutronReturnThread(Thread):
    def __init__(
        self, group=None, target=None, name=None, args=(), kwargs={}, verbose=None
    ):
        super().__init__(group, target, name, args, kwargs)
        self._return = None

    def run(self):
        if self._target is not None:
            self._return = self._target(*self._args, **self._kwargs)

    def join(self):
        super().join()
        return self._return


def whereis_pid(name):
    def worker():
        return enif_whereis_pid(name)

    t1 = CommutronReturnThread(target=worker)
    t1.start()
    return t1.join()
