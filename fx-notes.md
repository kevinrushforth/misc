# Processing JavaFX Properties

A _property_ is defined implicitly by a no-args method whose name ends in `Property` and whose
return type is a subtype of `javafx.beans.Observable`.  
(The return type check can be weakened for JDK testing.)

The name of the property is the part of the method name before `Property`.
The type of the property is the return type of the method.

A property may have a like-named field of the appropriate type.

A property may have _getter_ and _setter_ methods. The _getter_ method may be named `is...`
for boolean properties.

There is no element that _directly_ represents the property. 
Generally, the property method is used in the code as a proxy for the property, 
since it is the one element that is always present.

## Getter, Setter and Property Methods

1. In the constructor for `MemberSummaryBuilder`, a `PropertyHelper` is created.
   
   * First, the element to be used for the description of the property is determined.
     The field is used if it is present; the property method is used if there is no
     field, or if the field does not have a comment.
   
   * Then, a `classPropertiesMap` is created for methods that may need additional processing.
     
     * The getter and setter methods are added if they do not have an existing comment.
     (If they have a comment, the methods are not subject to any additional processing.)
       
     * The property method is added if it does not have a comment or if it is the element 
     to be used for the property description. This means an existing comment on the 
     property method may be subject to additional processing.
     (If both the field and property method have comments, the property method is not
     subject to any additional processing.)
     
2. In `MemberSummaryBuilder::buildSummary`, if an element is registered in the `classPropertiesMap`
   it is passed to `MemberSummary::processProperty` to perform any additional processing,
   before any documentation related to the element is generated.
   
3. In `MemberSummary::processProperty`, comments on the property method, getter and setter
   are created or updated. Note this is only called for methods that were previously
   registered in the `classPropertiesMap` 
   (And so it is not called for getter and setter methods that already have a comment,
   or for the property method if it has a comment and the field also has a comment.)
   
   Generally, this method works to build a new comment (body and tags) to be set as
   the comment for the element.
   
   * For getter and setter methods, simple text is created for the body of the comment.
     An internal `@propertyDescription` tag is added that will subsequently cause the
     description of the property to be added to the method's documentation.
     
     For the property method, the text is taken from the element for the description.
     This will be the text from the field if the field has a comment and the property 
     method does not, or from itself if the field does not have a comment. 
     (If both have a comment, this method will not be called.)
     
   * `@since` and `@defaultValue` tags are copied from the element for the description.

   * _Bug:_ if the element for the description is the property method, any `@return` tag
     will _not_ be copied into the new comment, and so will be omitted from the generated
     documentation.
     
   * For the property method, `@see` tags will be added for the getter and setter.
    
   * _RFE:_ add `@see` tags for each of the getter, setter, and property methods to each 
     of the  getter, setter and property methods (excluding self-referential tags, of course.)
     
   * _RFE:_ add `@param` and `@return` tags as appropriate to each of the getter, setter,
     and property methods.
     
   The body and tags described above will be set for the element, overriding any existing
   values. This may mean other tags may be lost from the property method when it is the
   element for the description of the property. This is in addition to the bug previously
   noted about losing the `@return` tag.  For example, any additional `@see` tags will be
   lost.
   
## The Field

No special processing is done for the field, if any, associated with the property.


## The Property   

The description for the property is always taken from the property method.
This may be the synthesized content generated previously, if the field 
was present and had a comment, and the property method did not have content.

_Note:_ this can lead to a discrepancy when both the field and property method
have comments: the getter and setter will use the comment from the _field_,
but the property will use the comment from the _property method_.

_Bug:_ if the property method has (as it should) a `@return` tag, this will
inappropriately show up in the documentation for the property.

## Statistics

The following statistics are for the [JavaFX code base], July 2021.


| Group                                                    | Count |
|:---------------------------------------------------------|------:|
| classes with properties                                  |  1266 | 
| field comment, no prop method comment                    |   790 |
| field comment and prop method comment                    |    38 |
| no field or no field comment, prop method comment        |   400 |
| no field or no field comment and no prop method comment  |    38 |

It is a numerical coincidence that 38 classes have both a field comment and a property method comment,
and that 38 other classes have neither.

[JavaFX code base]: https://github.com/openjdk/jfx
   
