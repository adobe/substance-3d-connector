""" Outlines the connection schema template for established connections """
import json
import jsonschema

SCHEMA = {
    "type": "object",
    "properties": {
        "display_name": {"type": "string"},
        "id_name": {"type": "string"},
        "connector_version": {"type": "string"},
        "available_features": {"type": "string"},
    },
}


class ConnectionSchema:
    """ Schema for descriptor information about each connection """
    def __init__(self, display_name, id_name, connector_version, available_features):
        self.display_name = display_name
        self.id_name = id_name
        self.connector_version = connector_version
        self.available_features = available_features

    def to_json(self):
        """ Returns a json string of this object"""
        return json.dumps(self, default=lambda o: o.__dict__, sort_keys=True,
                          indent=4)

    def is_valid(self):
        """ Returns a boolean if the json is valid """
        try:
            json_filler = json.loads(self.to_json())
            jsonschema.validate(instance=json_filler, schema=SCHEMA)
            return True
        except jsonschema.exceptions.ValidationError as err:
            print("Json error: {}".format(err))

        return False

