class Tree < ActiveRecord::Base
  
  belongs_to :planet
  
  # tell plist library how to render a message in a plist.
  # puts all the relevant information in a hash, then renders that as a plist fragment.
  # the fragments are collected and wrapped in the plist boilerplate in the controller.
  def to_plist_node
    hash = { :x => x, :y => y, :z => z, :age => age, :frequency => frequency }
    Plist::Emit.dump(hash, false)
  end
  
end