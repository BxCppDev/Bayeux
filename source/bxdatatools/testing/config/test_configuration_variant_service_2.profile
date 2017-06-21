#@format=datatools::configuration::variant
#@format.version=1.0
#@organization=bxdatatools
#@application=test_configuration_variant_service_2

# bar = "abc"

[registry="management"]
user = "Samantha"
role = "maintainer"
phase = "Commissioning"
my_plugin = false
logging = "debug"
#bar = "abc"

[registry="foo"]
bar = "abc"
baz = 42

[registry="geometry"]
layout = "Unshielded"
OM_model = "10''"
