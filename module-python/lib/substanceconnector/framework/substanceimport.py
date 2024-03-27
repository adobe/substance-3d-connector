""" Application type to Handle all importing of Substance file types, such
    as sbsar, sbs and sbsprs files """

import uuid

from .instance import ConnectorInstance
from .application import BaseApplication


class SubstanceImportApplication(BaseApplication):
    """ Handles all importing and exporting of sbsar, sbs and sbsprs files """
    _IMPORT_SBSAR_UUID = uuid.UUID('72538d04-276f-4254-a45b-d3654f705477')

    @classmethod
    def get_callback_list(cls):
        """ Returns the callback list for substance file imports """
        return [(cls._IMPORT_SBSAR_UUID, cls.recv_import_sbsar)]\
            + super().get_callback_list()

    @classmethod
    def recv_import_sbsar(cls, context, message_type, message):
        """ Import the sbsar into Substance Designer """

    @classmethod
    def send_import_sbsar(cls, context, message):
        """ Send an sbs/sbsar file to another application """
        ConnectorInstance.write_message(context, cls._IMPORT_SBSAR_UUID, message)

